ALTER TABLE listings DROP CONSTRAINT IF EXISTS fk_website_id;
ALTER TABLE listings ADD CONSTRAINT fk_website_id FOREIGN KEY (website_id) REFERENCES websites(id);