CLI = docker-compose
ENV_FILE_PRIVATE = .env
DOCKER_COMPOSE_FILE = docker-compose.yml

COMMAND = $(CLI) --env-file $(ENV_FILE_PRIVATE) -f $(DOCKER_COMPOSE_FILE)

start:
	$(COMMAND) up -d

logs:
	$(COMMAND) logs --follow $(SERVICE)

attach:
	$(COMMAND) exec $(SERVICE) /bin/sh

stop:
	$(COMMAND) down

.PHONY: start logs attach stop

