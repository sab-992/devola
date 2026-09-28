CREATE TABLE IF NOT EXISTS tasks (
    uuid         UUID UNIQUE NOT NULL DEFAULT uuidv7(),
    user_uuid    UUID NOT NULL,
    status       VARCHAR NOT NULL,
    started_at   TIMESTAMP NOT NULL DEFAULT now(),
    last_updated_at TIMESTAMP NOT NULL DEFAULT now(),
    PRIMARY KEY (uuid, user_uuid)
);