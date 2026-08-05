CREATE OR REPLACE PROCEDURE insert_listings_batch(payload jsonb)
LANGUAGE plpgsql AS $$
BEGIN
    INSERT INTO listings (website_id, title, category, company, location, publication, content, link, expire_at)
    SELECT x.website_id,
           x.title,
           x.category,
           x.company,
           x.location,
           to_timestamp(x.publication),
           x.content,
           x.link,
           to_timestamp(x.expire_at)
    FROM jsonb_to_recordset(payload) AS x(website_id  BIGINT,
                                          title       VARCHAR,
                                          category    VARCHAR,
                                          company     VARCHAR,
                                          location    VARCHAR,
                                          publication DOUBLE PRECISION,
                                          content     TEXT,
                                          link        VARCHAR,
                                          expire_at   DOUBLE PRECISION);
END;
$$;