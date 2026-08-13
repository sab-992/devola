CREATE TABLE IF NOT EXISTS websites (
    host          VARCHAR(253) NOT NULL,
    endpoint      VARCHAR(255) NOT NULL,
    last_updated_at TIMESTAMP NOT NULL DEFAULT now(),
    PRIMARY KEY (host, endpoint)
);