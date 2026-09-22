CREATE OR REPLACE PROCEDURE insert_listings_batch(payload jsonb, INOUT result jsonb DEFAULT NULL)
LANGUAGE plpgsql
AS $$
BEGIN
    WITH inserted AS (INSERT INTO listings (website_host,
                                            website_endpoint,
                                            title,
                                            category,
                                            company,
                                            location,
                                            content,
                                            link,
                                            created_at,
                                            expire_at)
                      SELECT x.website_host,
                             x.website_endpoint,
                             x.title,
                             x.category,
                             x.company,
                             x.location,
                             x.content,
                             x.link,
                             to_timestamp(x.created_at),
                             to_timestamp(x.expire_at)
                      FROM jsonb_to_recordset(payload) AS x(website_host     VARCHAR(253),
                                                            website_endpoint VARCHAR(255),
                                                            title            VARCHAR,
                                                            category         VARCHAR,
                                                            company          VARCHAR,
                                                            location         VARCHAR,
                                                            content          TEXT,
                                                            link             VARCHAR,
                                                            created_at       DOUBLE PRECISION,
                                                            expire_at        DOUBLE PRECISION)
                      RETURNING *)
    SELECT coalesce(jsonb_agg(to_jsonb(inserted) || jsonb_build_object('created_at', extract(epoch FROM inserted.created_at),
                                                                       'expire_at',  extract(epoch FROM inserted.expire_at))), '[]'::jsonb)
    INTO result
    FROM inserted;
END;
$$;