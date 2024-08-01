from pythonosc import dispatcher
from pythonosc import osc_server

def sound_played_handler(unused_addr, args):
    print(f"Sound played: {args}")

if __name__ == "__main__":
    dispatcher = dispatcher.Dispatcher()
    dispatcher.map("/test", sound_played_handler)

    ip = "192.168.1.37"  # Adjust this if your receiver is on a different machine
    port = 3333      # Make sure this matches the port you set in the openFrameworks app

    server = osc_server.BlockingOSCUDPServer((ip, port), dispatcher)
    print(f"Serving on {ip}:{port}")
    server.serve_forever()
