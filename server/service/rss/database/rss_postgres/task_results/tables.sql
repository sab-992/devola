CREATE TABLE IF NOT EXISTS task_results (
    id           BIGINT GENERATED ALWAYS AS IDENTITY UNIQUE PRIMARY KEY,
    user_uuid    UUID NOT NULL,
    task_uuid    UUID NOT NULL,
    result       JSONB -- JSON STRUCTURE: { "listing_ids": [...], "scores": [[{"tag": "...", "score": ... }], ...]}
);