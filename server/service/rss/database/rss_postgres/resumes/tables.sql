CREATE TABLE IF NOT EXISTS resumes (
    user_uuid    UUID NOT NULL,
    tag          VARCHAR(255) NOT NULL,
    content      TEXT NOT NULL,
    created_at   TIMESTAMP NOT NULL DEFAULT now(),
    last_updated_at TIMESTAMP NOT NULL DEFAULT now(),
    PRIMARY KEY (tag, user_uuid)
);