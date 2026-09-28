ALTER TABLE refresh_tokens

DROP CONSTRAINT IF EXISTS fk_refresh_tokens_user_uuid,
ADD CONSTRAINT fk_refresh_tokens_user_uuid FOREIGN KEY (user_uuid) REFERENCES users(uuid);