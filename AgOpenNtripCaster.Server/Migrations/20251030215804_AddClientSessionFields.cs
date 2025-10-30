using Microsoft.EntityFrameworkCore.Migrations;

#nullable disable

namespace AgOpenNtripCaster.Server.Migrations
{
    /// <inheritdoc />
    public partial class AddClientSessionFields : Migration
    {
        /// <inheritdoc />
        protected override void Up(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.AddColumn<string>(
                name: "ClientIpAddress",
                table: "ClientSessions",
                type: "text",
                nullable: true);

            migrationBuilder.AddColumn<int>(
                name: "SerialNumber",
                table: "ClientSessions",
                type: "integer",
                nullable: false,
                defaultValue: 0);
        }

        /// <inheritdoc />
        protected override void Down(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.DropColumn(
                name: "ClientIpAddress",
                table: "ClientSessions");

            migrationBuilder.DropColumn(
                name: "SerialNumber",
                table: "ClientSessions");
        }
    }
}
