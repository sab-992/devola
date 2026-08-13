ALTER TABLE task_results

DROP CONSTRAINT IF EXISTS fk_task_results_task_uuid,
ADD CONSTRAINT fk_task_results_task_uuid FOREIGN KEY (task_uuid) REFERENCES tasks(uuid);