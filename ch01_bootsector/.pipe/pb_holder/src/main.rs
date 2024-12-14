use std::env;
use std::fs::{File, OpenOptions};
use std::io::{Read, Write};
use std::os::windows::fs::OpenOptionsExt; // Needed for named pipes on Windows
use flate2::{Compression, write::ZlibEncoder, read::ZlibDecoder};

const BUFFER_SIZE: usize = 512;

fn write_mode(pipe_name: &str, output_file: &str) -> std::io::Result<()> {
    // Open the named pipe for reading
    let mut pipe = OpenOptions::new()
        .read(true)
        .write(false)
        .custom_flags(0x40000000) // FILE_FLAG_FIRST_PIPE_INSTANCE
        .open(pipe_name)?;

    let mut buffer = [0u8; BUFFER_SIZE];
    pipe.read_exact(&mut buffer)?; // Read 512 bytes from the pipe

    // Compress the data
    let mut compressed_data = Vec::new();
    let mut encoder = ZlibEncoder::new(&mut compressed_data, Compression::default());
    encoder.write_all(&buffer)?;
    encoder.finish()?;

    // Save the compressed data to a file
    let mut file = File::create(output_file)?;
    file.write_all(&compressed_data)?;

    println!("Data compressed and saved to {output_file}");
    Ok(())
}

fn read_mode(input_file: &str, pipe_name: &str) -> std::io::Result<()> {
    // Open the compressed file
    let mut file = File::open(input_file)?;

    let mut compressed_data = Vec::new();
    file.read_to_end(&mut compressed_data)?;

    // Decompress the data
    let mut decoder = ZlibDecoder::new(&compressed_data[..]);
    let mut buffer = [0u8; BUFFER_SIZE];
    decoder.read_exact(&mut buffer)?;

    // Write the decompressed data to the named pipe
    let mut pipe = OpenOptions::new()
        .read(false)
        .write(true)
        .custom_flags(0x40000000) // FILE_FLAG_FIRST_PIPE_INSTANCE
        .open(pipe_name)?;

    pipe.write_all(&buffer)?;
    println!("Data decompressed and written to the pipe");
    Ok(())
}

fn main() -> std::io::Result<()> {
    let args: Vec<String> = env::args().collect();

    if args.len() < 4 {
        eprintln!("Usage:");
        eprintln!("  {} write (or w) <pipe_name> <output_file>", args[0]);
        eprintln!("  {} read (or r) <input_file> <pipe_name>", args[0]);
        std::process::exit(1);
    }

    match args[1].as_str() {
        "write" | "w" => write_mode(&args[2], &args[3])?,
        "read" | "r" => read_mode(&args[2], &args[3])?,
        _ => {
            eprintln!("Invalid mode. Use 'write' or 'read'.");
            std::process::exit(1);
        }
    }

    Ok(())
}
