# Use official Python 3.10 image
FROM python:3.10

# Set the working directory inside the container
WORKDIR /app

# Copy requirements and install dependencies
COPY requirements.txt .
RUN pip install --no-cache-dir -r requirements.txt

# Copy AI training scripts and config files
COPY ai_source/ ai_source/
COPY executable/ executable/

# Set the default command to run AI training
CMD ["python", "ai_source/train.py"]
