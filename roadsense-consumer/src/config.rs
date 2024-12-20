use dotenv::from_path;
use std::env;

const KEYS: [&str; 11] = [
    "RABBIT_HOST",
    "RABBIT_PORT",
    "RABBIT_USER",
    "RABBIT_PASSWORD",
    "RABBIT_QUEUE",
    "RABBIT_EXCHANGE",
    "CONSUMER_NAME",
    "SUPPORTED_CONTENT_TYPE",
    "DATABASE_URL",
    "QUEUE_SIZE",
    "QUEUE_TIMEOUT_SECS",
];

pub fn load_env() -> bool {
    let root_dir = env::current_dir().expect("Failed to get current directory");
    let dotfile = root_dir.join(".env");

    // Resolve the absolute path to the `.env` file in the parent directory
    println!("Loading config from {:?}", dotfile);

    // Load the .env file from the specified path
    if let Err(e) = from_path(&dotfile) {
        println!("[!] Failed to load .env file: {}", e);
        return false;
    }

    // Ensure all keys are present
    let mut ok = true;
    for key in KEYS {
        match env::var(key) {
            Ok(_v) => {}
            Err(_e) => {
                println!("[!] Config {} not present.", key);
                ok = false;
            }
        }
    }

    ok
}
