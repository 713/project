#!/usr/bin/env python

import sqlite3
from datetime import datetime
import re

#===============================================================================
# Temporary variables
#===============================================================================
user_email = "luigileung@gmail.com"
user_id = 0
start_time = datetime.now()

#===============================================================================

# Open connection and get a cursor (pointer)
conn = sqlite3.connect(':memory:')
c = conn.cursor()

# Create schema for a new table with db_name 'user_email_table'
c.execute('CREATE TABLE IF NOT EXISTS table_users (user_id INTEGER, user_email TEXT, start_time TEXT)')
conn.commit()

# Insert user's email (via webpage's button onClick or form action)
c.execute('INSERT INTO table_users values (?, ?, ?) ', (user_id, user_email, start_time))
conn.commit()

c.execute('INSERT INTO table_users values (?, ?, ?) ', (1, 'hello', '2'))
conn.commit()

# Gets user's email
print(c.execute('SELECT * FROM table_users WHERE user_id = ?', (0,)))  # needs ,

row = list(c)[0]
print(row)

# Test
print(c.execute('SELECT * FROM table_users'))  # needs ,
print(list(c))
print(list(c))  # has to select table again before showing up.
print(c.execute('SELECT * FROM table_users'))  # needs ,
print(list(c)[0])
print(list(c))  # has to select table again even did not show all items in query

print("NEXT QUERY")
print(c.execute('SELECT start_time FROM table_users WHERE user_email = ?', ("hello",)))  # needs ,
user_email = list(c)
#user_email = re.search('\'(.+?)\'', user_email) # extracts email from query
#if not user_email:
#    print("Email not found")

