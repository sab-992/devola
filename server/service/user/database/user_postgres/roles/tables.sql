CREATE TABLE IF NOT EXISTS roles (
    name        VARCHAR(255) PRIMARY KEY,
    description VARCHAR(255)
);

INSERT INTO roles (name, description) VALUES ('user', 'Standard user'),
                                             ('admin', 'Administrator') ON CONFLICT (name) DO NOTHING;
