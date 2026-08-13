CREATE OR REPLACE FUNCTION get_task_results(p_task_uuid UUID)
RETURNS TABLE (
    listing_id BIGINT,
    website_host VARCHAR,
    website_endpoint VARCHAR,
    company VARCHAR,
    title VARCHAR,
    link VARCHAR,
    scores JSONB,
    status VARCHAR,
    started_at TIMESTAMP,
    last_updated_at TIMESTAMP
) AS $$
BEGIN
    RETURN QUERY
    SELECT
        (listing_id_elem->>'')::BIGINT AS listing_id,
        l.website_host,
        l.website_endpoint,
        l.company,
        l.title,
        l.link,
        score_group,
        t.status,
        t.started_at,
        t.last_updated_at
    FROM task_results tr
    JOIN tasks t ON tr.task_uuid = t.uuid
    CROSS JOIN LATERAL jsonb_array_elements(tr.result) AS result_item(item)
    CROSS JOIN LATERAL jsonb_array_elements(result_item.item->'listing_ids') 
        WITH ORDINALITY AS listing_ids(listing_id_elem, listing_idx)
    CROSS JOIN LATERAL jsonb_array_elements(result_item.item->'scores') 
        WITH ORDINALITY AS scores_groups(score_group, score_idx)
    JOIN listings l ON l.id = (listing_id_elem->>'')::BIGINT
    WHERE tr.task_uuid = p_task_uuid
    AND listing_ids.listing_idx = scores_groups.score_idx;
END;
$$ LANGUAGE plpgsql;