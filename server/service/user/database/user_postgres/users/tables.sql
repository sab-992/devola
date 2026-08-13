CREATE TABLE IF NOT EXISTS users (
    uuid              UUID UNIQUE NOT NULL DEFAULT uuidv7(),
    username          VARCHAR(255) NOT NULL,
    email             VARCHAR(255) NOT NULL UNIQUE,
    password          VARCHAR(255) NOT NULL, -- Hashed password
    first_name        VARCHAR(100),
    last_name         VARCHAR(100),
    is_active         BOOLEAN DEFAULT true,
    is_email_verified BOOLEAN DEFAULT false,
    created_at        TIMESTAMP DEFAULT now(),
    last_updated_at        TIMESTAMP DEFAULT now(),
    last_login        TIMESTAMP,
    PRIMARY KEY (uuid, username)
);