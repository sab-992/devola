GRANT CONNECT ON DATABASE devola TO devola_app;

CREATE SCHEMA devola AUTHORIZATION devola_app;
ALTER ROLE devola_app SET search_path = devola;
ALTER ROLE devola_app SET timezone TO 'UTC';