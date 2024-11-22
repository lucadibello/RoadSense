CLI = docker-compose
ENV_FILE = .env
DOCKER_COMPOSE_FILE = docker-compose.yml
SERVICE = traefik

COMMAND = $(CLI) --env-file $(ENV_FILE) -f $(DOCKER_COMPOSE_FILE)

start:
	$(COMMAND) up -d $(SERVICE)

logs:
	$(COMMAND) logs --follow $(SERVICE)

attach:
	$(COMMAND) exec $(SERVICE) /bin/sh

stop:
	$(COMMAND) down

.PHONY: start logs attach stop

