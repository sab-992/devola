CREATE TABLE IF NOT EXISTS user_roles (
    user_uuid UUID UNIQUE NOT NULL,
    role VARCHAR(255),
    PRIMARY KEY (user_uuid, role)
);