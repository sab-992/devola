CREATE TABLE IF NOT EXISTS websites (
    id          BIGINT GENERATED ALWAYS AS IDENTITY UNIQUE,
    host        VARCHAR(253) NOT NULL,
    endpoint    VARCHAR(255) NOT NULL,
    added       TIMESTAMPTZ NOT NULL DEFAULT now(),
    PRIMARY KEY (host, endpoint)
);