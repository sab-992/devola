CREATE OR REPLACE PROCEDURE insert_listings_batch(payload jsonb)
LANGUAGE plpgsql AS $$
BEGIN
    INSERT INTO listings (website_host, website_endpoint, title, category, company, location, publication, content, link)
    SELECT x.website_host,
           x.website_endpoint,
           x.title,
           x.category,
           x.company,
           x.location,
           to_timestamp(x.publication),
           x.content,
           x.link
    FROM jsonb_to_recordset(payload) AS x(website_host     VARCHAR(253),
                                          website_endpoint VARCHAR(255),
                                          title            VARCHAR,
                                          category         VARCHAR,
                                          company          VARCHAR,
                                          location         VARCHAR,
                                          publication      DOUBLE PRECISION,
                                          content          TEXT,
                                          link             VARCHAR);
END;
$$;