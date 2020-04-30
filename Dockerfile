FROM python:3.8

RUN apt-get update && apt-get install -y swig

ADD . /openudm
WORKDIR /openudm

RUN python setup.py build
RUN python setup.py install

# Test a model
ENTRYPOINT ["python", "scripts/udm_solo.py"]
