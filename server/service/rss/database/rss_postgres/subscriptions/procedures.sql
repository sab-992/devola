CREATE OR REPLACE PROCEDURE save_subscriptions(p_user_uuid UUID, p_subscriptions JSON)
AS $$
    BEGIN
        DELETE FROM subscriptions s
        WHERE s.user_uuid = p_user_uuid
        AND NOT EXISTS (SELECT 1 FROM json_to_recordset(p_subscriptions) AS x(host VARCHAR(253),
                                                                            endpoint VARCHAR(255))
                                WHERE x.host = s.website_host
                                AND x.endpoint = s.website_endpoint);

        INSERT INTO subscriptions (user_uuid, website_host, website_endpoint)
        SELECT p_user_uuid, x.host, x.endpoint
        FROM json_to_recordset(p_subscriptions) AS x(host VARCHAR(253), endpoint VARCHAR(255))
        ON CONFLICT (user_uuid, website_host, website_endpoint) DO NOTHING;
    END;
$$ LANGUAGE plpgsql;