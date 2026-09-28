CREATE TABLE IF NOT EXISTS resumes (
    user_uuid    UUID NOT NULL,
    tag          VARCHAR(255) NOT NULL,
    content      TEXT NOT NULL,
    skills       TEXT NOT NULL,
    last_updated_at TIMESTAMP NOT NULL DEFAULT now(),
    PRIMARY KEY (tag, user_uuid)
);