import os
import json
import traceback 

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
LOG_DIR = os.path.join(BASE_DIR, '..', 'app', 'Model') 
LOG_FILE = os.path.join(LOG_DIR, 'json_requests.log')

if not os.path.exists(LOG_DIR):
    os.makedirs(LOG_DIR)

def log_request(log_data):
    with open(LOG_FILE, 'a') as log_file:
        log_file.write(json.dumps(log_data) + "\n")

def log_error(error):
    error_data = {
        "type": "error",
        "error_message": str(error),
        "stack_trace": traceback.format_exc()
    }
    with open(LOG_FILE, 'a') as log_file:
      log_file.write(json.dumps(error_data) + "\n")

def delete_log_file():
    if os.path.exists(LOG_FILE):
        os.remove(LOG_FILE)
        print(f"Arquivo de log {LOG_FILE} apagado.")

