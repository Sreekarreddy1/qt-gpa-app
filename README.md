# Qt GPA App: C++ Client/Server on Docker and Kubernetes

A small client/server project that shows the full path from C++ code to a container running on Kubernetes.

- **Server:** plain C++ TCP server (POSIX sockets). It reads a line of marks and replies with the average GPA.
- **Client:** a Qt Widgets GUI (`QTcpSocket`) that runs on the desktop. A PowerShell client is also used for testing.
- **Deployment:** the server is packaged with Docker and deployed to minikube with a Deployment and a Service.

## Architecture

```
 [Client: Qt GUI / PowerShell]
            |
            |  TCP :5000
            v
 [kubectl port-forward]  -->  [Service: gpa-server]  -->  [Pod(s): C++ server]
```

Inside the cluster, other pods reach the server by name (`gpa-server:5000`), and the Service spreads connections across all replicas.

## Project structure

```
qt-gpa-app/
├── server/
│   ├── server.cpp      # TCP server (no Qt, Linux sockets)
│   ├── Dockerfile      # multi-stage build: gcc -> debian slim
│   └── k8s.yaml        # Deployment + Service
└── client/
    ├── CMakeLists.txt  # Qt6 Widgets + Network
    ├── main.cpp
    ├── mainwindow.h
    └── mainwindow.cpp  # GUI + QTcpSocket logic
```

## Protocol

| Direction | Message |
|---|---|
| Client -> Server | one line of numbers, e.g. `8 9 7.5` |
| Server -> Client | `Average GPA: 8.166667` |

## Run the server with Docker

```
cd server
docker build -t gpa-server:1.0 .
docker run -p 5000:5000 gpa-server:1.0
```

Test from PowerShell:

```powershell
$c = New-Object System.Net.Sockets.TcpClient("127.0.0.1", 5000)
$s = $c.GetStream()
$w = New-Object IO.StreamWriter($s)
$w.WriteLine("8 9 7.5"); $w.Flush()
(New-Object IO.StreamReader($s)).ReadLine()
$c.Close()
```

Result: `Average GPA: 8.166667`

## Deploy to minikube

```
minikube start
minikube image load gpa-server:1.0
kubectl apply -f k8s.yaml
kubectl get pods
kubectl port-forward svc/gpa-server 5000:5000
```

Test through the port-forward with the same PowerShell snippet (`10 9 8 7` returned `Average GPA: 8.500000`), then check the pod:

```
kubectl logs deploy/gpa-server
```

## Scaling and load balancing

```
kubectl scale deploy/gpa-server --replicas=2
```

`kubectl port-forward` stays on a single pod, so traffic is sent from inside the cluster instead, where the Service balances it:

```
kubectl run tester --rm -it --image=busybox --restart=Never -- sh -c 'for i in 1 2 3 4 5 6; do echo 8 9 $i | nc gpa-server 5000; done'
```

Observed result (6 requests):

| Pod | Requests handled |
|---|---|
| `gpa-server-...-6kg6j` | `8 9 4` (plus an earlier port-forward test) |
| `gpa-server-...-qd5r8` | `8 9 1`, `8 9 2`, `8 9 3`, `8 9 5`, `8 9 6` |

Both pods served traffic. The split is random per connection, so it is not always even.

## Run the Qt client

Requirements: Qt 6 (Widgets + Network), CMake, a C++17 compiler.

1. Open `client/CMakeLists.txt` in Qt Creator and select a Qt 6 kit.
2. Run the app.
3. Use host `127.0.0.1`, port `5000`, enter marks, and click **Send to server**.

The client works the same against the plain Docker container or the Kubernetes Service (through port-forward). Nothing on the server side changes.

## What this project demonstrates

- C++ socket programming (server) and Qt signals/slots (client)
- Multi-stage Docker builds
- Kubernetes Deployment, Service, scaling, and in-cluster service discovery
- Testing and debugging with `kubectl logs`, `port-forward`, and a temporary test pod

## Clean up

```
kubectl delete -f k8s.yaml
```
