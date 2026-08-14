ALTER TABLE subscriptions

DROP CONSTRAINT IF EXISTS fk_subscriptions_website_host_endpoint,
ADD CONSTRAINT fk_subscriptions_website_host_endpoint FOREIGN KEY (website_host, website_endpoint) REFERENCES websites(host, endpoint);