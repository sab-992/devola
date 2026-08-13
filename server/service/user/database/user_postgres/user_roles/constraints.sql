ALTER TABLE user_roles

DROP CONSTRAINT IF EXISTS fk_user_roles_user_uuid,
ADD CONSTRAINT fk_user_roles_user_uuid FOREIGN KEY (user_uuid) REFERENCES users(uuid),

DROP CONSTRAINT IF EXISTS fk_user_roles_role,
ADD CONSTRAINT fk_user_roles_role FOREIGN KEY (role) REFERENCES roles(name);