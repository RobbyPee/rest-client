import requests
import hashlib
import os
import time
from datetime import datetime, timedelta

def fetch_data(url):
    """Fetch data from the specified URL."""
    try:
        print("Sending request...")
        response = requests.get(url)
        response.raise_for_status()  # Raise an exception for HTTP errors
        
        raw_text = response.text
        print(f"Response received: {len(raw_text)} characters")
        return raw_text
    except requests.exceptions.RequestException as e:
        print(f"Error fetching data: {e}")
        return None

def save_unique_response(raw_text, seen_hashes, output_dir="responses"):
    """Save raw response as a .json file if it hasn't been seen before."""
    if raw_text is None:
        return False
    
    # Create hash of the raw text to check for uniqueness
    data_hash = hashlib.md5(raw_text.encode()).hexdigest()
    
    # Check if we've seen this response before
    if data_hash in seen_hashes:
        print("Response already saved, skipping...")
        return False
    
    # Create output directory if it doesn't exist
    os.makedirs(output_dir, exist_ok=True)
    
    # Generate a filename with timestamp and hash
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    filename = f"{output_dir}/response_{timestamp}_{data_hash[:8]}.json"
    
    # Save the raw text directly to a .json file
    try:
        with open(filename, 'w') as f:
            f.write(raw_text)
        
        print(f"Unique response saved to {filename}")
        seen_hashes.add(data_hash)
        return True
    except Exception as e:
        print(f"Error saving file: {e}")
        return False

def main():
    url = "http://test.brightsign.io:3000"
    seen_hashes = set()
    query_count = 0
    unique_count = 0
    
    # Set time limit (10 minutes)
    start_time = datetime.now()
    end_time = start_time + timedelta(minutes=10)
    
    # Set unique response limit (20)
    max_unique_responses = 20
    
    print(f"Starting to query {url}")
    print(f"Will run until {end_time.strftime('%H:%M:%S')} or until {max_unique_responses} unique responses are collected")
    
    try:
        while datetime.now() < end_time and unique_count < max_unique_responses:
            query_count += 1
            current_time = datetime.now()
            time_elapsed = (current_time - start_time).total_seconds() / 60
            time_remaining = (end_time - current_time).total_seconds() / 60
            
            print(f"\nQuery #{query_count} at {current_time.strftime('%H:%M:%S')}")
            print(f"Time elapsed: {time_elapsed:.2f} minutes, Time remaining: {time_remaining:.2f} minutes")
            
            raw_response = fetch_data(url)
            if raw_response:
                is_unique = save_unique_response(raw_response, seen_hashes)
                if is_unique:
                    unique_count += 1
                    print(f"Unique responses: {unique_count}/{max_unique_responses}")
            
            # Wait before making the next request
            if datetime.now() < end_time and unique_count < max_unique_responses:
                print("Waiting 5 seconds before next request...")
                time.sleep(5)  # 5 second pause between requests
            
    except KeyboardInterrupt:
        print("\nProgram stopped by user")
    
    # Final statistics
    run_time = (datetime.now() - start_time).total_seconds() / 60
    reason = "time limit reached" if datetime.now() >= end_time else "collected enough unique responses"
    
    print("\n========== Final Results ==========")
    print(f"Program completed: {reason}")
    print(f"Total runtime: {run_time:.2f} minutes")
    print(f"Total queries: {query_count}")
    print(f"Unique responses collected: {unique_count}")
    print("==================================")

if __name__ == "__main__":
    main()