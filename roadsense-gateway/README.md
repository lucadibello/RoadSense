# RoadSense Gateway Documentation

This document provides an overview of the Traefik reverse proxy setup for the RoadSense project, including router configurations, local certificate generation using `mkcert`, and managing services such as RabbitMQ.

---

## Overview

The RoadSense gateway is configured using **Traefik** as a reverse proxy to manage HTTP, HTTPS, and AMQP traffic. This ensures secure and efficient routing between the services within the platform. The configuration supports local development with `mkcert` for TLS certificates and includes routing for:

- The Traefik dashboard.
- RabbitMQ management UI.
- AMQP protocol for message queuing.

---

## Table of Contents

1. [Services Overview](#services-overview)
2. [Environment Setup](#environment-setup)
3. [TLS Certificates with `mkcert`](#tls-certificates-with-mkcert)
4. [Routing Configuration](#routing-configuration)
5. [Makefile Commands](#makefile-commands)
6. [Logs and Debugging](#logs-and-debugging)

---

## Services Overview

### 1. **Traefik**

- Manages routing for HTTP, HTTPS, and AMQP traffic.
- Provides a secure dashboard to monitor routing configurations.
- Routes are defined in `docker-compose.yml` using Traefik labels.

### 2. **RabbitMQ**

- Provides a message queue service with both a management UI and AMQP protocol access.
- Configured with separate HTTP and TCP routers for management and message traffic.

---

## Environment Setup

Ensure you have the following files in your project directory:

- `.env`: Private environment variables.
- `.env.local`: Public environment variables.

### Sample `.env` Variables:

```env
DOMAIN=roadsense.local
RABBITMQ_DEFAULT_USER=admin
RABBITMQ_DEFAULT_PASS=secret
CERTIFICATE_DOMAINS=roadsense.dev,traefik.roadsense.dev,rabbit.roadsense.dev
```

---

## TLS Certificates with `mkcert`

To enable HTTPS for local development, `mkcert` is used to generate self-signed certificates.

### Steps:

1. Install `mkcert` on your system:
   ```bash
   brew install mkcert   # macOS
   sudo apt install mkcert   # Ubuntu
   ```
2. Run the provided script:
   ```bash
   ./generate-certificates.sh
   ```
3. Certificates are stored in the `certs` directory and automatically mounted into the Traefik container.

### Example Certificate Mapping in `traefik.yml`:

```yaml
tls:
  certificates:
    - certFile: /certs/roadsense.dev+1.pem
      keyFile: /certs/roadsense.dev+1-key.pem
```

---

## Routing Configuration

### Traefik Routers

#### Main HTTPS Router

```yaml
traefik.http.routers.traefik.rule=Host(`traefik.${DOMAIN}`)
traefik.http.routers.traefik.entrypoints=websecure
traefik.http.routers.traefik.tls=true
traefik.http.routers.traefik.service=api@internal
```

#### RabbitMQ Management UI

```yaml
traefik.http.routers.rabbitmq-ui.rule=Host(`rabbit.${DOMAIN}`)
traefik.http.routers.rabbitmq-ui.entrypoints=websecure
traefik.http.routers.rabbitmq-ui.tls=true
traefik.http.services.rabbitmq-ui.loadbalancer.server.port=15672
```

#### RabbitMQ AMQP (TCP Traffic)

```yaml
traefik.tcp.routers.rabbitmq-amqp.entrypoints=amqp
traefik.tcp.routers.rabbitmq-amqp.rule=HostSNI(`*`)
traefik.tcp.routers.rabbitmq-amqp.tls=false
traefik.tcp.services.rabbitmq-amqp.loadbalancer.server.port=5672
```

### HTTP to HTTPS Redirection

Configured in `traefik.yml`:

```yaml
entryPoints:
  web:
    address: ":80"
    http:
      redirections:
        entryPoint:
          to: websecure
          scheme: https
  websecure:
    address: ":443"
    http:
      tls: {}
```

---

## Makefile Commands

The provided `Makefile` simplifies project management:

- **Start the project**:

  ```bash
  make start
  ```

- **View logs**:

  ```bash
  make logs SERVICE=<service_name>
  ```

- **Attach to a service shell**:

  ```bash
  make attach SERVICE=<service_name>
  ```

- **Stop the project**:
  ```bash
  make stop
  ```

---

## Logs and Debugging

### Access Logs

Traefik logs all access requests in `/traefik/logs/access.log`:

```yaml
accessLog:
  filePath: "/traefik/logs/access.log"
  format: common
```

### Enable Debug Mode

Debugging is enabled via:

```yaml
log:
  level: DEBUG
```

View live logs:

```bash
docker logs -f traefik
```

---

## Testing the Setup

1. Add the following entries to your `/etc/hosts` file for local testing:
   ```
   127.0.0.1 traefik.roadsense.local
   127.0.0.1 rabbit.roadsense.local
   ```
2. Start the services:
   ```bash
   make start
   ```
3. Access:
   - Traefik Dashboard: `https://traefik.roadsense.local`
   - RabbitMQ UI: `https://rabbit.roadsense.local`

---

This setup ensures that RoadSense services are securely and efficiently routed using Traefik and local TLS certificates.
