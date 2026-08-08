CREATE TABLE IF NOT EXISTS task_results (
    id           BIGINT GENERATED ALWAYS AS IDENTITY UNIQUE PRIMARY KEY,
    user_uuid    UUID UNIQUE NOT NULL,
    task_uuid    UUID NOT NULL,
    website_host VARCHAR(253) NOT NULL,
    website_endpoint VARCHAR(255) NOT NULL,
    result       JSONB -- JSON STRUCTURE: { "listing_ids": [...], "scores": [[{"tag": "...", "score": ... }], ...]}
);