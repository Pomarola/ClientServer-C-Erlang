-module(servidor).

-export([start/1]).
-export([clientMap/1,worker/1]).

%% inicio del servidor
start(Port)->
    io:format("Servidor Iniciado.~n"),
    register(map_proc, spawn(?MODULE, clientMap, [(maps:new())])),
    case gen_tcp:listen(Port, [list, {active, false}]) of
        {ok, ListenSocket} -> listenLoop(ListenSocket);
        {error, Reason} -> io:format("Fallo al escuchar: ~p ~n", [Reason])
    end,
    Port.

%% loop para aceptar incoming clients
listenLoop(ListenSocket)->
    
    case gen_tcp:accept(ListenSocket) of
        {ok, CSocket} ->
            spawn(?MODULE, worker, [CSocket]);
        {error, closed} ->
            io:format("Se cerró el closed, nos vamos a mimir"),
            exit(normal);
        {error, Reason} ->
            io:format("Falló la espera del client por: ~p~n",[Reason])

    end,
    listenLoop(ListenSocket).

%% map con los clientes del servidor actuales con sus respectivos sockets y un handler
clientMap(CMap) ->
    receive
        %% Crea un nuevo cliente en el map
        {newClient, Name, Socket} ->
            clientMap(maps:put(Name, Socket ,CMap));

        %% Intenta encontrar el nickname en el map y devuelve el socket o -1 si no lo encuentra
        {searchCliSock, Name, WorkID} ->
            case maps:get(Name, CMap, -1) of
                -1 -> WorkID ! {err, notfound};
                Value -> WorkID ! {ok, Value}
            end,

            clientMap(CMap);    

        %% Intenta cambiar el nickname del map
        {changeNick, NewName, OldName, Socket, WorkID} ->
            case maps:find(NewName, CMap) of
                {ok, _Name} -> 
                    WorkID ! {err, ocupied},
                    clientMap(CMap);
                error -> 
                    WorkID ! {ok, changed},
                    UpdatedMap = maps:remove(OldName, CMap),
                    clientMap(maps:put(NewName, Socket ,UpdatedMap))
            end;

        %% Envia al trabajador todos los sockets del map
        {allCliSock, WorkID} ->
            WorkID ! {ok, maps:values(CMap)},
            clientMap(CMap);

        %% Elimina un cliente del map segun su nombre
        {deleteCli, Name} ->
            clientMap(maps:remove(Name, CMap))

    end.

worker(CSocket) ->
    gen_tcp:send(CSocket, "Para comenzar a chatear ingrese un Nickname\0"),
    Name = workerAskNick(CSocket),
    map_proc ! {newClient, Name, CSocket},
    workerLoop(CSocket, Name).


workerAskNick(CSocket) ->
    case gen_tcp:recv(CSocket, 0) of
        {ok, Buff} ->
            [Data, _] = string:split(Buff,[0]),
            map_proc ! {searchCliSock, Data, self()},
            receive 
                {ok, _Value} -> 
                    gen_tcp:send(CSocket, "Nickname ocupado intente con otro\0"),
                    workerAskNick(CSocket);
                {err, notfound} ->
                    gen_tcp:send(CSocket, "Bienvenido, ya puede comenzar a chatear\0"),
                    Data
            end;
        {error, Reason} ->
            io:format("Falló la espera del client por: ~p~n",[Reason])
    end.

workerLoop(ClientSocket, Name) ->
    case gen_tcp:recv(ClientSocket, 0) of
        {ok, Buff} ->
            [Data, _] = string:split(Buff,[0]),
            case getAction(Data) of
                {gen, Msg} -> %%msg general

                    map_proc ! {allCliSock, self()},
                    receive
                        {ok, Sockets} -> 
                            lists:map(fun(X) -> gen_tcp:send(X,"[" ++ Name ++ "] dice: " ++ Msg ++ [0]) end, Sockets)
                    end,
                    workerLoop(ClientSocket, Name);

                {priv, DestName, Msg} -> %%msg priv
                    map_proc ! {searchCliSock, DestName, self()},
                    receive
                        {ok, DestSock} ->  
                            gen_tcp:send(DestSock, "[Privado] [" ++ Name ++ "] dice: " ++ Msg ++ [0]);
                        {err, notfound} ->
                            gen_tcp:send(ClientSocket, "No se reconoce al nickname con el que se esta intentando chatear\0")
                    end,
                    workerLoop(ClientSocket, Name);

                {nick, NewName} -> %% nickname change
                    map_proc ! {changeNick, NewName, Name, ClientSocket, self()},
                    receive 
                        {err, ocupied} -> 
                            gen_tcp:send(ClientSocket, "Nickname ocupado intente nuevamente\0"),
                            workerLoop(ClientSocket, Name);
                        {ok, changed} ->
                            gen_tcp:send(ClientSocket, "Nickname cambiado!\0"),
                            workerLoop(ClientSocket, NewName)
                    end;
            
                cliExit -> %%exit
                    map_proc ! {deleteCli, Name},
                    gen_tcp:send(ClientSocket, "Hasta luego!\0"),
                    exit(normal);

                unrec -> %%unrecognized
                    gen_tcp:send(ClientSocket, "Comando no reconocido\0"),
                    workerLoop(ClientSocket, Name)

            end;
        {error, closed} ->
            io:format("El cliente cerró la conexión~n");
        {error, Reason} ->
            io:format("Falló la espera del client por: ~p~n",[Reason])
   end.

getAction(Data) ->
    case Data of
        "/msg " ++ Buff->
            [DestName, Msg] = string:split([Buff], " "),
            {priv, DestName, Msg};
        "/nickname " ++ Nickname-> 
            {nick, Nickname};
        "/exit" ++ _ -> 
            cliExit;
        "/" ++ _ ->
            unrec;
        Msg ->
            {gen, Msg}
    end.
