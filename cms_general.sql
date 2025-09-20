CREATE TABLE cms_general_users (
UserId INT AUTO INCRENEMT NOT NULL,
country_id INT,
general_file_id INT,
UserName VARCHAR,
Email VARCHAR,
status INT,
inactive INT,
firsta_name VARCHAR,
last_name VARCHAR,
created TIMRSTAMP NOT NULL CURRENT_TIMESTAMP,
login TIMRSTAMP,
AccessFailedCount INT NOT NULL 0
PRIMARY KEY (UserId)
);

CREATE TABLE cms_general_country (
country_id INT AUTO INCRENEMT NOT NULL,
country_name VARCHAR
PRIMARY KEY (country_id)
);

CREATE TABLE cms_general_file (
general_file_id INT AUTO INCRENEMT NOT NULL,
UserId INT,
file_name VARCHAR,
file_uri VARCHAR,
file_mime VARCHAR,
file_blob BLOB
PRIMARY KEY (general_file_id)
);

CREATE TABLE cms_general_user_role (
UserId INT,
RoleId INT
);

CREATE TABLE cms_general_role (
RoleId INT AUTO INCRENEMT NOT NULL,
name VARCHAR NOT NULL,
is_sysadmin INT,
description VARCHAR
PRIMARY KEY (RoleId)
);

CREATE TABLE cms_general_permissions (
pid INT AUTO INCRENEMT NOT NULL,
mid INT,
permissions VARCHAR,
description VARCHAR
PRIMARY KEY (pid)
);

CREATE TABLE cms_general_role_permissions (
pid INT,
RoleId INT
);

CREATE TABLE cms_general_module (
mid INT AUTO INCRENEMT NOT NULL,
module NOT NULL,
description NOT NULL,
active_module NOT NULL
PRIMARY KEY (mid)
);

ALTER TABLE cms_general_users ADD CONSTRAINT 
FOREIGN KEY (country_id,general_file_id,)
REFERENCES cms_general_country (country_id,)
ON DELETE NO ACTION
ON UPDATE NO ACTION

ALTER TABLE cms_general_users ADD CONSTRAINT 
FOREIGN KEY (country_id,general_file_id,)
REFERENCES cms_general_file (general_file_id,)
ON DELETE NO ACTION
ON UPDATE NO ACTION

ALTER TABLE cms_general_file ADD CONSTRAINT 
FOREIGN KEY (UserId,)
REFERENCES cms_general_users (UserId,)
ON DELETE NO ACTION
ON UPDATE NO ACTION

ALTER TABLE cms_general_user_role ADD CONSTRAINT 
FOREIGN KEY (UserId,RoleId,)
REFERENCES cms_general_users (UserId,)
ON DELETE NO ACTION
ON UPDATE NO ACTION

ALTER TABLE cms_general_user_role ADD CONSTRAINT 
FOREIGN KEY (UserId,RoleId,)
REFERENCES cms_general_role (RoleId,)
ON DELETE NO ACTION
ON UPDATE NO ACTION

ALTER TABLE cms_general_role_permissions ADD CONSTRAINT 
FOREIGN KEY (pid,RoleId,)
REFERENCES cms_general_permissions (pid,)
ON DELETE NO ACTION
ON UPDATE NO ACTION

ALTER TABLE cms_general_role_permissions ADD CONSTRAINT 
FOREIGN KEY (pid,RoleId,)
REFERENCES cms_general_role (RoleId,)
ON DELETE NO ACTION
ON UPDATE NO ACTION

ALTER TABLE cms_general_permissions ADD CONSTRAINT 
FOREIGN KEY (mid,)
REFERENCES cms_general_module (mid,)
ON DELETE NO ACTION
ON UPDATE NO ACTION

