FROM postgres:17.4

COPY ./sql/ /files/sql

RUN chown -R postgres:postgres /files/sql && \
    chmod -R 755 /files/sql && \
    find /files/sql -type f -name "*.sql" -exec mv {} /docker-entrypoint-initdb.d/ \; && \
    chown -R postgres:postgres /docker-entrypoint-initdb.d/ && \
    chmod -R 755 /docker-entrypoint-initdb.d/

# We don't need to expose the default PostgreSQL port 5432, our back-end can simply connect to the address (of the docker name in the docker compose file) and the port 5432.
CMD ["postgres"]