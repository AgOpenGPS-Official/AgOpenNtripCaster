-- Get the admin user ID
SELECT id FROM "AspNetUsers" WHERE "Email" = 'admin@ntripcaster.local';

-- Get the Admin role ID
SELECT id FROM "AspNetRoles" WHERE "Name" = 'Admin';

-- Add admin user to Admin role (replace IDs as needed)
-- INSERT INTO "AspNetUserRoles" ("UserId", "RoleId") 
-- VALUES ('{USER_ID}', '{ROLE_ID}');
