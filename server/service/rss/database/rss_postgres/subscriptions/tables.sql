CREATE TABLE IF NOT EXISTS subscriptions (
    user_uuid        UUID NOT NULL,
    website_host     VARCHAR(253) NOT NULL,
    website_endpoint VARCHAR(255) NOT NULL,
    created_at       TIMESTAMP NOT NULL DEFAULT now(),
    PRIMARY KEY (user_uuid, website_host, website_endpoint)
);