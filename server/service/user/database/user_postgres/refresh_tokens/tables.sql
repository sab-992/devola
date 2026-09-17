CREATE TABLE IF NOT EXISTS refresh_tokens (
    token      TEXT,
    user_uuid  UUID NOT NULL,
    created_at TIMESTAMP NOT NULL,
    expire_at  TIMESTAMP NOT NULL,
    revoked    BOOLEAN DEFAULT false,
    PRIMARY KEY (token, user_uuid)
);