
FROM gcc:13 AS build

WORKDIR /src

COPY server.cpp .

RUN g++ -O2 -static -o server server.cpp

FROM debian:bookworm-slim

COPY --from=build /src/server /server

EXPOSE 5000

CMD ["/server"]