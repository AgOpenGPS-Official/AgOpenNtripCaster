# Multi-stage build for ASP.NET 9 Core
FROM mcr.microsoft.com/dotnet/sdk:9.0 AS build
WORKDIR /src

COPY ["NtripCaster.Server.csproj", "./"]
RUN dotnet restore "NtripCaster.Server.csproj"

COPY . .
RUN dotnet build "NtripCaster.Server.csproj" -c Release -o /app/build

FROM build AS publish
RUN dotnet publish "NtripCaster.Server.csproj" -c Release -o /app/publish

FROM mcr.microsoft.com/dotnet/aspnet:9.0 AS final
WORKDIR /app

# Install curl for health checks
RUN apt-get update && apt-get install -y curl && rm -rf /var/lib/apt/lists/*

COPY --from=publish /app/publish .

EXPOSE 5000
EXPOSE 2101

ENTRYPOINT ["dotnet", "NtripCaster.Server.dll"]
