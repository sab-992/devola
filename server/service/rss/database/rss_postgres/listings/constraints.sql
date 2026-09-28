ALTER TABLE listings

DROP CONSTRAINT IF EXISTS fk_listings_website_host_endpoint,
ADD CONSTRAINT fk_listings_website_host_endpoint FOREIGN KEY (website_host, website_endpoint) REFERENCES websites(host, endpoint);