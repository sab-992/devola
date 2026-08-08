CREATE TABLE IF NOT EXISTS listings (
    id               BIGINT GENERATED ALWAYS AS IDENTITY UNIQUE PRIMARY KEY,
    website_host     VARCHAR(253) NOT NULL,
    website_endpoint VARCHAR(255) NOT NULL,
    title            VARCHAR NOT NULL,
    category         VARCHAR NOT NULL,
    company          VARCHAR NOT NULL,
    location         VARCHAR NOT NULL,
    publication      TIMESTAMP NOT NULL,
    content          TEXT NOT NULL,
    link             VARCHAR NOT NULL
);