CREATE OR REPLACE PROCEDURE set_last_updated(p_host VARCHAR(253), p_endpoint VARCHAR(255))
AS $$
    BEGIN
        UPDATE website
        SET last_updated_at = now()
        WHERE host = p_host AND endpoint = p_endpoint;
    COMMIT;
END;
$$ LANGUAGE plpgsql;


CREATE OR REPLACE PROCEDURE insert_websites(p_websites JSON)
AS $$
    BEGIN
        INSERT INTO websites (host, endpoint) SELECT x.host, x.endpoint
                                              FROM json_to_recordset(p_websites) AS x(host VARCHAR(253), endpoint VARCHAR(255))
        ON CONFLICT (host, endpoint) DO NOTHING;
END;
$$ LANGUAGE plpgsql;