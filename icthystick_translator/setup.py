setup(
    name="icthystick translator",
    version="1.0.0",
    description="Converts Bluetooth BLE messages to UART",
    long_description=README,
    long_description_content_type="text/markdown",
    url="https://github.com/AcBotics-Public",
    author="Acbotics Research",
    author_email="",
    license="MIT",
    classifiers=[
        "License :: OSI Approved :: MIT License",
        "Programming Language :: Python",
        "Programming Language :: Python :: 3",
    ],
    packages=["icthystick_translate"],
    include_package_data=True,
    install_requires=[
        "bleak", "pyserisal"
    ],
    entry_points={"console_scripts": ["icthystick_translator=icthystick_translate.icthystick_translate:main"]},
)
