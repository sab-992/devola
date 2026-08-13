-- TODO: Uncomment when user table is added
-- ALTER TABLE task_results DROP CONSTRAINT IF EXISTS fk_task_results_user_uuid;
-- ALTER TABLE task_results ADD CONSTRAINT fk_task_results_user_uuid FOREIGN KEY (user_uuid) REFERENCES user(uuid);

ALTER TABLE task_results

DROP CONSTRAINT IF EXISTS fk_task_results_task_uuid,
ADD CONSTRAINT fk_task_results_task_uuid FOREIGN KEY (task_uuid) REFERENCES tasks(uuid);