CREATE OR REPLACE PROCEDURE set_last_updated(p_host VARCHAR(253), p_endpoint VARCHAR(255))
AS $$
    BEGIN
        UPDATE website
        SET last_updated_at = now()
        WHERE host = p_host AND endpoint = p_endpoint;
    COMMIT;
END;
$$ LANGUAGE plpgsql;