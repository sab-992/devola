CREATE TABLE IF NOT EXISTS websites (
    host          VARCHAR(253) NOT NULL,
    endpoint      VARCHAR(255) NOT NULL,
    last_updated TIMESTAMP NOT NULL DEFAULT now(),
    expire_at    TIMESTAMP NOT NULL,
    PRIMARY KEY (host, endpoint)
);