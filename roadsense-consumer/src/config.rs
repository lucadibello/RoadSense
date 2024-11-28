use dotenv::from_path;
use log::{error, info};
use std::env;

const KEYS: [&str; 5] = [
    "RABBIT_HOST",
    "RABBIT_PORT",
    "RABBIT_USER",
    "RABBIT_PASSWORD",
    "RABBIT_QUEUE",
];

pub fn load_env() -> bool {
    let root_dir = env::current_dir().expect("Failed to get current directory");
    let dotfile = root_dir.join(".env");

    // Resolve the absolute path to the `.env` file in the parent directory
    info!("Loading config from {:?}", dotfile);

    // Load the .env file from the specified path
    if let Err(e) = from_path(&dotfile) {
        error!("Failed to load .env file: {}", e);
        return false;
    }

    // Ensure all keys are present
    let mut ok = true;
    for key in KEYS {
        match env::var(key) {
            Ok(_v) => {}
            Err(_e) => {
                error!("Config {} not present.", key);
                ok = false;
            }
        }
    }

    return ok;
}
