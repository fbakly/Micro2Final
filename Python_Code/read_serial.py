import serial
import mysql.connector
import datetime
import schedule
from mysql.connector import Error
from multiprocessing import Process

def read_and_send():
    try:
        ser = serial.Serial('/dev/ttyACM0')
        conn = mysql.connector.connect(user='fbakly', password='F0u@d871999',
                host='localhost', database='Micro2Final')
        if conn.is_connected():
            lux = int.from_bytes(ser.read(2), byteorder='big', signed=False)
            cursor = conn.cursor()
            current_dt = datetime.datetime.now()
            dt_string = current_dt.strftime("%Y-%m-%d %H:%M:%S")
            query = '''INSERT INTO payload VALUES (NULL, "''' + dt_string + '''", ''' + str(lux) + ''');'''
            cursor.execute(query)
            conn.commit()
            cursor.close()
    except Error as e:
        print(e)
    except Exception as ex:
        print(ex)
    finally:
        if (conn.is_connected()):
            conn.close()
            ser.close()

def run_schedule():
    schedule.every(10).minutes.do(read_and_send)
    while True:
        schedule.run_pending()

if __name__ == "__main__":
    try:
        stop = False
        print("Running...")
        p = Process(target=run_schedule)
        p.start()
        while not stop:
            user_input = input("Enter 'q' to quit: ")
            if user_input == 'q':
                stop = True
            else:
                print("Unkown Command")
    except Exception as ex:
        print(ex)
    finally:
        if p.is_alive():
            p.terminate()
        print("Exiting")
