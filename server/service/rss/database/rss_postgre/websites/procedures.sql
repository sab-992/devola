CREATE OR REPLACE FUNCTION set_last_updated()
RETURNS TRIGGER AS $$
BEGIN
    NEW.last_updated = now();
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE TRIGGER trg_websites_last_updated
BEFORE UPDATE ON websites
FOR EACH ROW
EXECUTE FUNCTION set_last_updated();