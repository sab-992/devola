FROM postgres:17.4

RUN chown -R postgres:postgres /docker-entrypoint-initdb.d/ && \
    chmod -R 755 /docker-entrypoint-initdb.d/

# We don't need to expose the default PostgreSQL port 5432, our back-end can simply connect to the address (of the docker name in the docker compose file) and the port 5432.
CMD ["postgres"]